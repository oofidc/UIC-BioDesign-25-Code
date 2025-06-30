import requests
from io import BytesIO
import PIL
from PIL import Image
from skimage.feature import graycomatrix, graycoprops
from skimage.measure import shannon_entropy
import numpy as np
import skimage.feature
import skimage.measure

class img_feature_analysis:
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
        
#print(img_feature_analysis.calculate_glcm_feature('Blue_Printer_Paper.png','contrast')) #Extremely Low Contrast
#print(img_feature_analysis.calculate_glcm_feature('High_Contrast.png','contrast')) # Extremely High Contrast

