import serial
import time
import pandas as pd
import xgboost as xgb
import joblib

# ---------- LOAD MODEL & LABEL ENCODER ----------
model = xgb.Booster()
model.load_model("milk_xgb.json")        # trained model
le = joblib.load("label_encoder.pkl")    # label encoder

# ---------- SERIAL CONNECTION ----------
# CHANGE COM PORT ACCORDING TO YOUR SYSTEM
ser = serial.Serial('COM3', 9600, timeout=1)
time.sleep(2)  # wait for Arduino reset

print("Live Milk Quality Prediction Started...")
print("Waiting for Arduino data...\n")

# ---------- LIVE PREDICTION LOOP ----------
while True:
    line = ser.readline().decode('utf-8').strip()

    if line:
        try:
            # Expected format from Arduino:
            # pH,Temperature,Odor,Turbidity,Colour
            ph, temp, odor, turbidity, colour = map(float, line.split(","))

            sample = pd.DataFrame([{
                'pH': ph,
                'Temprature': temp,   # keep SAME spelling as training
                'Odor': odor,
                'Turbidity': turbidity,
                'Colour': colour
            }])

            dmatrix = xgb.DMatrix(sample, enable_categorical=True)
            prediction = model.predict(dmatrix)

            milk_grade = le.inverse_transform([int(prediction[0])])[0]
            print(f"Sensor Data → {line}")
            print(f"Predicted Milk Quality → {milk_grade}\n")

        except Exception as e:
            print("Data error:", line)
