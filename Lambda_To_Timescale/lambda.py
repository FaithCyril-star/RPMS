import psycopg2

conn_str = "postgresql://user:password:port/database"
conn = psycopg2.connect(conn_str)
cursor = conn.cursor()

def handler(event,context):
    try:
        query = f"INSERT INTO DeviceData (timestamp, device_id, temperature, heart_rate, oxygen_saturation, systolic_pressure, diastolic_pressure)  VALUES (NOW(),'{event['device_id']}', {event['temperature']}, {event['heart_rate']}, {event['oxygen_saturation']}, {event['systolic_pressure']}, {event['diastolic_pressure']});"
        cursor.execute(query)  # Execute the SQL query
        conn.commit()
        return f"{event} inserted successfully"
    except Exception as e:
        return e




