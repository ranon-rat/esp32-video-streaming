#define PART_BOUNDARY "123456789000000000000987654321"
#define STREAM_CONTENT_TYPE  "multipart/x-mixed-replace;boundary=" PART_BOUNDARY
#define STREAM_BOUNDARY  "\r\n--" PART_BOUNDARY "\r\n"
#define STREAM_PART "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n"