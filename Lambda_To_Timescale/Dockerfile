FROM public.ecr.aws/lambda/python:3.8

# Copy the Python code and requirements file
COPY lambda.py .
COPY requirements.txt .

# Install psycopg2
RUN pip install -r requirements.txt

# Set the Lambda function handler
CMD ["lambda.handler"]
