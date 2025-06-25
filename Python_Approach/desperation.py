from asyncio import sleep
import math
import requests
import serial
import random
from sklearn.neighbors import KNeighborsRegressor
from sklearn.model_selection import train_test_split
from skimage.feature import graycomatrix, graycoprops
from sklearn.metrics import mean_squared_error, mean_absolute_error, r2_score
from skimage.measure import shannon_entropy
import numpy as np
from PIL import Image
import requests
from io import BytesIO
import pandas as pd
import matplotlib.pyplot as plt

#Initialize Serial
com_port = 'COM9'  #Changeable com port
baud_rate = 9600
try:
    ser = serial.Serial(com_port, baud_rate)
except:
    print("Error Opening serial Port")


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
def calculate_glcm_entropy(image_path):
    try:
        img = Image.open(image_path).convert('L')
        image_as_arr = np.array(img)
        return skimage.measure.shannon_entropy(image_as_arr)
    except Exception as e:
        print(f"Error: {e}")
        return None
def calculate_glcm_homogenity(image_path):
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
        homogenity = graycoprops(glcm, 'homogeneity')[0, 0]
        return homogenity
    except Exception as e:
        print(f"Error: {e}")
        return None
def calculate_glcm_energy(image_path):
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
        homogenity = graycoprops(glcm, 'energy')[0, 0]
        return homogenity
    except Exception as e:
        print(f"Error: {e}")
        return None
def calculate_glcm_feature(image_path,feature):
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
        r = graycoprops(glcm, feature)[0, 0]
        return r
    except Exception as e:
        print(f"Error: {e}")
        return None



def train_and_init_model():
    prod_df = pd.read_csv("image_data.csv")
    X = prod_df[['Contrast','Entropy','Energy','Correlation']]
    y = prod_df['Roughness']


    # Split the data into training and testing sets
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)


    neigh = KNeighborsRegressor(n_neighbors=5)
    neigh.fit(X_train, y_train)

    y_pred = neigh.predict(X_test)

    
    # Calculate accuracy metrics
    mse = mean_squared_error(y_test, y_pred)
    r2 = r2_score(y_test, y_pred)
    mae = mean_absolute_error(y_test,y_pred)

    print(f"Mean Squared Error: {mse:.2f}")
    print(f"R-squared: {r2:.2f}")
    print(f"Mean Absolute Error: {mae:.2f}")
    print(f"")

    return neigh
    
trained_model = train_and_init_model()

def trigger_vib_voltage(predicted_roughness):

    


    print(f"Random variable: {predicted_roughness}")
    #quit()
    vibrational_output = int(1023/(1+pow(math.e,-10*(predicted_roughness-3)))
)
    # Write the random variable to the serial port
    
        
    ser.write(str(int(vibrational_output)).encode())
    print(f"Random variable {vibrational_output} written to {com_port}")

def predict_roughness_from_url(url, trained_model):
    """
    Predicts the roughness of a surface based on an image captured from a URL.

    Parameters:
        url (str): The URL to capture the image.
        trained_model: The trained model for roughness prediction.

    Returns:
        float: The predicted roughness value.
    """

    # Make a request to get the image
    print("REQUESTING DATA\n")
    response = requests.get(url)
    if response.status_code == 200:
        with open('captured_image.jpg', 'wb') as file:
            file.write(response.content)
        print("Image saved as captured_image.jpg")
    else:
        print("Failed to capture image")
        return None

    # Load the image and calculate GLCM features
    test_image_path = 'captured_image.jpg'
    contrast_value = calculate_glcm_contrast(test_image_path)
    entropy_value = calculate_glcm_feature(test_image_path,'entropy')
    homogeneity_value = calculate_glcm_homogenity(test_image_path)
    energy_value = calculate_glcm_energy(test_image_path)
    correlation_value = calculate_glcm_feature(test_image_path, 'correlation')

    # Create a DataFrame for the test image features
    test_data = pd.DataFrame([{
        'Contrast': contrast_value,
        'Entropy': entropy_value,
        'Energy': energy_value,
        'Correlation': correlation_value
    }])
    print(test_data)

    # Predict the roughness rating using the trained model
    predicted_roughness = trained_model.predict(test_data)

    print(f"Predicted Roughness for the captured image: {predicted_roughness[0]}")
    return predicted_roughness[0]



# Example usage

#url = 'https://img.freepik.com/free-photo/white-png-base_23-2151645368.jpg?t=st=1745789603~exp=1745793203~hmac=e60e0fffdcf725c9ed2e47204077a79fd550c337c382ba76449da9e335c38e15&w=1380'
#url = 'https://freerangestock.com/sample/42943/close-up-of-a-cluster-of-rocks.jpg'
#predicted_roughness = predict_roughness_from_url(url, trained_model)
import time
url = 'http://192.168.4.1/capture'
time.sleep(3)
pred_0 = 0 #Previous predicted roughness
while(True):
    predicted_roughness = predict_roughness_from_url(url, trained_model)
    if(pred_0 > predicted_roughness):
        temp = predicted_roughness
        predicted_roughness = pred_0
        pred_0 = temp
    else:
        pred_0 = predicted_roughness
    trigger_vib_voltage(predicted_roughness=predicted_roughness)
    time.sleep(1)
   




'''
#quit()
x = 1
# Write the random variable to the serial port
try:
    ser = serial.Serial(com_port, baud_rate)
    time.sleep(3)
    ser.write(str(int(x)).encode())
    print(f"Variable x written")
except serial.SerialException as e:
        print(f"Error: {e}")
ser.close()'''

    

#Begin Training Model
ser.close() # Close serial at absolute end of program
'''
# URL to capture image
url = 'http://192.168.4.1/capture'

# Make a request to get the image
print("REQUESTING DATA\n")
response = requests.get(url)
if response.status_code == 200:
	with open('captured_image.jpg', 'wb') as file:
		file.write(response.content)
	print("Image saved as captured_image.jpg")
else:
	print("Failed to capture image")


# Define the COM port and baud rate
com_port = 'COM9'  # Change this to your COM port
baud_rate = 9600

# Generate a random variable
random_variable = calculate_glcm_contrast('./captured_image.jpg')
print(f"Random variable: {random_variable}")
#quit()
(random_variable * 1023)/255
# Write the random variable to the serial port
try:
	ser = serial.Serial(com_port, baud_rate)
	
	ser.write(str(random_variable).encode())
	ser.close()
	print(f"Random variable {random_variable} written to {com_port}")
except serial.SerialException as e:
	print(f"Error: {e}")'''