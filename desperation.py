from asyncio import sleep
import requests
import serial
import random
from skimage.feature import graycomatrix, graycoprops
import numpy as np
from PIL import Image
import requests
from io import BytesIO

def calculate_glcm_contrast(image_path):
    """Calculates the contrast feature of the GLCM for a given image (local path or URL)."""
    try:
        # Check if the path is a URL
        if image_path.startswith(('http://', 'https://')):
            # Download the image from the URL
            response = requests.get(image_path)
            response.raise_for_status()  # Raise error for bad status codes
            img = Image.open(BytesIO(response.content)).convert('L')  # Convert to grayscale
        else:
            # Open local image
            img = Image.open(image_path).convert('L')

        image_array = np.array(img)
        glcm = graycomatrix(image_array, distances=[1], angles=[0], levels=256, symmetric=True, normed=True)
        contrast = graycoprops(glcm, 'contrast')[0, 0]
        return contrast

    except Exception as e:
        print(f"Error: {e}")
        return None


#VALUES CAN BE FROM 130 to 255


# URL to capture image
url = 'http://192.168.4.1/capture'
'''
# Make a request to get the image
response = requests.get(url)
if response.status_code == 200:
	with open('captured_image.jpg', 'wb') as file:
		file.write(response.content)
	print("Image saved as captured_image.jpg")
else:
	print("Failed to capture image")
'''

# Define the COM port and baud rate
com_port = 'COM9'  # Change this to your COM port
baud_rate = 9600

# Generate a random variable
random_variable = calculate_glcm_contrast('./High_Contrast.png')

# Write the random variable to the serial port
try:
	ser = serial.Serial(com_port, baud_rate)
	
	ser.write(str(random_variable).encode())
	ser.close()
	print(f"Random variable {random_variable} written to {com_port}")
except serial.SerialException as e:
	print(f"Error: {e}")