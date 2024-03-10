"""
A minimal API to speak OP_MSG to a MongoDB server.

"""

import bson
import socket
import struct

"""
Specified format of OP_MSG:

struct Section {
    uint8 payloadType;
    union payload {
        document  document; // payloadType == 0
        struct sequence {   // payloadType == 1
            int32      size;
            cstring    identifier;
            document*  documents;
        };
    };
};

struct OP_MSG {
    struct MsgHeader {
        int32  messageLength;
        int32  requestID;
        int32  responseTo;
        int32  opCode = 2013;
    };
    uint32      flagBits;
    Section+    sections;
    [uint32     checksum;]
};
"""
request_id = 0


def build_opmsg(
    payload0_document: bytes,
    payload1_identifier: str | None = None,
    payload1_documents: list[bytes] | None = None,
) -> bytes:
    """
    Builds an OP_MSG to send.
    """

    global request_id

    out = bytearray()

    # Serialize header.
    out += struct.pack("<i", 0)  # Placeholder for `messageLength`
    request_id += 1
    out += struct.pack("<i", request_id)  # requestID
    out += struct.pack("<i", 0)  # responseTo
    out += struct.pack("<i", 2013)  # opCode
    out += struct.pack("<I", 0)  # flagBits
    # Serialize payload 0 section
    out += struct.pack("<B", 0)
    out += payload0_document

    # Maybe serialize payload 1 section
    if payload1_identifier is not None:
        assert payload1_documents is not None
        out += struct.pack("<B", 1)
        size = 4  # length of size itself.
        size += len(payload1_identifier) + 1  # Include 1 for trailing null.
        size += sum([len(doc) for doc in payload1_documents])
        out += struct.pack("<i", size)  # size
        out += payload1_identifier.encode("utf8") + b"\0"  # identifier
        for doc in payload1_documents:
            out += doc  # documents

    out[0:4] = struct.pack("<i", len(out))  # Overwrite `messageLength`
    return out


def parse_opmsg(msg: bytes) -> bytes:
    """
    Parses an OP_MSG reply. Returns the payload 0 document.
    """
    _messageLength = struct.unpack("<i", msg[0:4])
    msg = msg[4:]
    _requestId = struct.unpack("<i", msg[0:4])
    msg = msg[4:]
    _responseTo = struct.unpack("<i", msg[0:4])
    msg = msg[4:]
    opCode = struct.unpack("<i", msg[0:4])[0]
    if opCode != 2013:
        raise Exception("Expected to parse opCode 2013, got: {}".format(opCode))
    msg = msg[4:]
    _flagBits = struct.unpack("<I", msg[0:4])
    msg = msg[4:]
    # Only expect one payload 0 section.
    payloadType = msg[0]
    if payloadType != 0:
        raise Exception("Expected to parse payloadType 0, got: {}".format(payloadType))
    msg = msg[1:]
    return msg


def _recvall(conn: socket.socket, want: int):
    """
    Receives exactly `want` bytes on a socket or raises an exception.
    """
    out = bytearray()
    while len(out) < want:
        got = conn.recv(want)
        if len(got) == 0:
            raise Exception("Received empty message on socket. Peer closed connection?")
        out += got
    return bytes(out)


def send_opmsg(
    conn: socket.socket,
    payload0_document: bytes,
    payload1_identifier: str | None = None,
    payload1_documents: list[bytes] | None = None,
) -> dict:
    # Send a hello.
    opmsg = build_opmsg(payload0_document, payload1_identifier, payload1_documents)
    conn.sendall(opmsg)

    # Receive reply.
    msg_size_le = _recvall(conn, 4)
    (msg_size,) = struct.unpack("<i", msg_size_le)
    remaining = _recvall(conn, msg_size - 4)
    msg = msg_size_le + remaining
    reply_bytes: bytes = parse_opmsg(msg)
    reply = bson.decode(reply_bytes)
    return reply


def connect() -> socket.socket:
    """
    Connects and does a MongoDB handshake. Returns a socket.
    """
    conn = socket.create_connection(("localhost", 27017), timeout=1.0)
    # Send handshake
    reply = send_opmsg(conn, bson.encode({"hello": 1, "$db": "admin"}))
    if reply["ok"] != 1:
        raise Exception("Did not get ok:1 in reply: {}".format(reply))
    return conn


maxBsonObjectSize = 16777216
maxMessageSizeBytes = 48000000
maxWriteBatchSize = 100000
allowance = 16 * 1024  # Refer: SERVER-10643
