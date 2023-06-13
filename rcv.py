import tensorflow
import numpy as np
import json
import socket

model = tensorflow.keras.models.load_model("./results/Best_model.h5")

class_mapping = ['thumbs_up','thumbs_down','ok','victory']

def MakePrediction(input):

    if not input.startswith("{\"rotations\":"):
        return "bad start", 0.0
    if not input.endswith("}]}"):
        return "bad end", 0.0
    if "}{" in input:
        return "two at a time", 0.0
        
    input_data = json.loads(input)

    pose = []

    for pose_data in input_data['rotations']:
        bone = [pose_data['roll'], pose_data['pitch'], pose_data['yaw']]
        pose.append(bone)

    pose = np.array(pose)
    pose = pose.flatten()
    pose = pose.reshape(1, 72, 1)

    prediction = model.predict(pose,verbose=0)

    prediction_list = np.argmax(prediction, axis=1)
    predicted_class = class_mapping[prediction_list[0]]
    probability_percentage = prediction[0,prediction_list[0]]
    probability_percentage = round(probability_percentage*100, 2).astype('str')

    print("Predicted class: " + predicted_class + " | Probability: " + probability_percentage + "%")

    return predicted_class, probability_percentage

IP = "127.0.0.1"
PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((IP, PORT))
sock.listen(5)

print("---Starting the predictor---")

while True:
    print("Waiting for connection...")
    connection, addr = sock.accept()
    print("Accepted connection.")

    while True:
        pose_data = connection.recv(64*64)
        if len(pose_data) == 0:
            break
        c, p = MakePrediction(pose_data)
        connection.sendall(f"{c};{p}".encode())
        
    connection.close()