/*
  by Yaser Ali Husen

  This code for sending data temperature and humidity to postresql database
  code based on pgconsole in SimplePgSQL example code.

*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SimplePgSQL.h>
#include "secrets.h"

//Setup connection and Database=====================
WiFiClient wifiClient;
IPAddress PGIP(IP_FIRST_OCTET,IP_SECOND_OCTET,IP_THIRD_OCTET,IP_FOURTH_OCTET);     // your PostgreSQL server IP
char buffer[1024];
char inbuf[128];
int pg_status = 0;
bool response;
PGconnection conn(&wifiClient, 0, 1024, buffer);



bool isDeviceRegistered()
{
  char *msg;
  int rc;
  if (!pg_status) {
    conn.setDbLogin(PGIP,
                    DATABASE_USER,
                    DATABASE_PASSWORD,
                    DATABASE_NAME,
                    "utf8");
    pg_status = 1;
    if (WiFi.status() == WL_CONNECTED) {
    sprintf(inbuf, "select * from devicemetadata where device_id='%s'",THINGNAME);      
    }
    else {
    Serial.println("WiFi Disconnected");}
    return false;;
  }

  if (pg_status == 1) {
    rc = conn.status();
    if (rc == CONNECTION_BAD || rc == CONNECTION_NEEDED) {
      char *c = conn.getMessage();
      if (c) Serial.println(c);
      pg_status = -1;
    }
    else if (rc == CONNECTION_OK) {
      pg_status = 2;
      Serial.println("Starting query");
    }
    return false;
  }
    
  
  
  if (pg_status == 2 && strlen(inbuf) > 0) {
    if (conn.execute(inbuf)) goto error;
    Serial.println("Working...");
    pg_status = 3;
    memset(inbuf, 0, sizeof(inbuf));
  }
  
  if (pg_status == 3) {
    rc = conn.getData();
    int i;
    if (rc < 0) goto error;
    if (!rc) return false;
    if (rc & PG_RSTAT_HAVE_COLUMNS) {
      for (i = 0; i < conn.nfields(); i++) {
        if (i) Serial.print(" | ");
        Serial.print(conn.getColumn(i));
      }
      Serial.println("\n==========");
    }
    else if (rc & PG_RSTAT_HAVE_ROW) {
      
      for (i = 0; i < conn.nfields(); i++) {
        if (i) Serial.print(" | ");
        msg = conn.getValue(i);
        if (!msg) msg = (char *)"NULL";
        Serial.print(msg);
      }
      Serial.println();
    }
    else if (rc & PG_RSTAT_HAVE_SUMMARY) {
      Serial.print("Rows affected: ");
      int rowsRetrieved = conn.ntuples();
      response = rowsRetrieved;
      Serial.println(rowsRetrieved);
    }
    else if (rc & PG_RSTAT_HAVE_MESSAGE) {
      msg = conn.getMessage();
      if (msg) Serial.println(msg);
    }
    if (rc & PG_RSTAT_READY) {
      pg_status = 2;
      Serial.println("Waiting query");
    }
  }
  return response;
error:
  msg = conn.getMessage();
  if (msg) Serial.println(msg);
  else Serial.println("UNKNOWN ERROR");
  if (conn.status() == CONNECTION_BAD) {
    Serial.println("Connection is bad");
    pg_status = -1;
  }
  return false;
}

