import tensorflow
import keras
import numpy
from PIL import Image #Python Environment lange instaliuoti ne PIL o Pillow, cia tas pats
import io
import socket
import time

#CNN_model = tensorflow.keras.models.load_model(".\Best_model_1.h5")

def make_a_prediction(image):

    gray_img = Image.open(io.BytesIO(image)).convert('L')

    newsize = (28, 28)
    gray_img = gray_img.resize(newsize)

    img_array = []
    img_array = numpy.asarray(gray_img)
    img_array = img_array / 255.0
    img_array = img_array.reshape(1,28,28,1)

    prediction = CNN_model.predict(img_array,verbose=0)

    prediction_list = numpy.argmax(prediction, axis=1)
    predicted_class = class_mapping[prediction_list[0]]
    print("Predicted class is " + predicted_class)

    probability_percentage = prediction[0,prediction_list[0]]
    probability_percentage = round(probability_percentage*100, 2).astype('str')
    print("Probability percentage is " + probability_percentage + "%")

    return predicted_class, probability_percentage

IP = "127.0.0.1"
PORT = 11000

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((IP, PORT))
sock.listen(5)

print("---Starting the predictor---")

while True:
    print("Waiting for connection...")
    connection, addr = sock.accept()
    print("Accepted connection.")

    while True:
        buf = connection.recv(64*64)
        if len(buf) == 0:
            break
        print(len(buf))
        print(buf.decode("utf-8"))
        #c, p = make_a_prediction(buf)
        timestamp = time.time_ns()
        with open(f"{timestamp}.png", "wb") as binary_file:
            binary_file.write(buf)
        with open(f"{timestamp}.txt", "w", encoding="utf-8") as result:
            result.write(f"{c};{p}")
        #connection.sendall(f"{c};{p}".encode())
        
    connection.close()