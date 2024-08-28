FROM alpine:3.19
RUN apk add build-base
COPY app /app
COPY dep /dep
COPY bson /bson
RUN cc -o libdep.so -shared dep/dep.c -I/bson
RUN cc -o app.out app/app.c -I/bson -L. -ldep -I/dep -Wl,-rpath .
ENTRYPOINT [ "./app.out" ]
