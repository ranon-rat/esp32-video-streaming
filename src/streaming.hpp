#include <WiFi.h>
#include "esp_camera.h"
#include "esp_http_server.h"
//#include <AsyncTCP.h>
//#include <ESPAsyncWebServer.h>
//

//macros

#define PART_BOUNDARY "123456789000000000000987654321"
#define STREAM_CONTENT_TYPE  "multipart/x-mixed-replace;boundary=" PART_BOUNDARY
#define STREAM_BOUNDARY  "\r\n--" PART_BOUNDARY "\r\n"
#define STREAM_PART "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n"
httpd_handle_t stream_httpd = NULL;

static esp_err_t streamHandler(httpd_req_t *req)
{
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  size_t jpg_buf_len = 0;
  uint8_t *jpg_buf = NULL;
  char *part_buf[64];

  res = httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
  if (res != ESP_OK)
  {
    return res;
  }

  while (true)
  {
    fb = esp_camera_fb_get();
    if (!fb)
    {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    }
    else
    {
      

        jpg_buf_len = fb->len;
        jpg_buf = fb->buf;
      
    }
    if (res == ESP_OK)
    {
      size_t hlen = snprintf((char *)part_buf, 64, STREAM_PART, jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if (res == ESP_OK)
    {
      res = httpd_resp_send_chunk(req, (const char *)jpg_buf, jpg_buf_len);
    }
    if (res == ESP_OK)
    {
      res = httpd_resp_send_chunk(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY));
    }
    if (fb)
    {
      esp_camera_fb_return(fb);
      fb = NULL;
      jpg_buf = NULL;
    }
    else if (jpg_buf)
    {
      free(jpg_buf);
      jpg_buf = NULL;
    }
    if (res != ESP_OK)
    {
      break;
    }
    Serial.printf("MJPG: %uB\n", (uint32_t)(jpg_buf_len));
  }
  return res;
}
static esp_err_t indexHandler(httpd_req_t *req)
{
    // this only send the homepage
    httpd_resp_set_type(req, "text/html");

    return httpd_resp_send(req, (const char *)show_camera_html, show_camera_html_len);
}
void startCameraServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    httpd_uri_t home_index = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = indexHandler,
        .user_ctx = NULL};

    httpd_uri_t img_trans = {
        .uri = "/img",
        .method = HTTP_GET,
        .handler = streamHandler,
        .user_ctx = NULL};

    //Serial.printf("Starting web server on port: '%d'\n", config.server_port);
    if (httpd_start(&stream_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd, &home_index);
        httpd_register_uri_handler(stream_httpd, &img_trans);
    }
}
