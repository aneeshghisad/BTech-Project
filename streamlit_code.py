import streamlit as st
import serial
import time
import pandas as pd
import xgboost as xgb
import joblib

# PAGE CONFIGURATIOn
st.set_page_config(page_title="Milk Quality Monitor", layout="wide")
st.title("🥛 Real-Time Milk Quality Detection System")

#  LOAD MODEL
@st.cache_resource
def load_model():
    model = xgb.Booster()
    model.load_model("milk_xgb.json")
    le = joblib.load("label_encoder.pkl")
    return model, le

model, le = load_model()

# SERIAL CONNECTION 
@st.cache_resource
def connect_serial():
    ser = serial.Serial('COM3', 9600, timeout=1)  # change if needed
    time.sleep(2)
    return ser

ser = connect_serial()

#UI BUTTON 
if st.button("🔄 Read Live Sensor Data"):

    line = ser.readline().decode('utf-8').strip()

    if line:
        try:
            ph, temp, odor, turbidity, colour = map(float, line.split(","))

            sample = pd.DataFrame([{
                'pH': ph,
                'Temprature': temp,  
                'Odor': odor,
                'Turbidity': turbidity,
                'Colour': colour
            }])

            dmatrix = xgb.DMatrix(sample, enable_categorical=True)
            prediction = model.predict(dmatrix)
            milk_grade = le.inverse_transform([int(prediction[0])])[0]

            # DISPLAY SENSOR VALUES
            st.subheader("📊 Sensor Data")
            st.dataframe(sample)

            #  DISPLAY RESULT 
            st.subheader("🧠 Prediction Result")

            if milk_grade.lower() == "good":
                st.success(f"Milk Quality: {milk_grade} ✅")
            elif milk_grade.lower() == "medium":
                st.warning(f"Milk Quality: {milk_grade} ⚠")
            else:
                st.error(f"Milk Quality: {milk_grade} ❌")

        except Exception:
            st.error("⚠ Data format error from Arduino")

    else:
        st.info("Waiting for data from Arduino...")