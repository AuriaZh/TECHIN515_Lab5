
from flask import Flask, request, jsonify
import tensorflow as tf
import numpy as np

app = Flask(__name__)

# Load trained model (wand_model.h5 should be in the same directory)
try:
    model = tf.keras.models.load_model("wand_model.h5")
    print("Model loaded successfully.")
except Exception as e:
    print(f"Failed to load model: {e}")
    model = None

@app.route('/', methods=['POST'])
def predict():
    if not model:
        return jsonify({"error": "Model not loaded."}), 500

    try:
        data = request.get_json(force=True)
        x = data.get('x', 0)
        y = data.get('y', 0)
        z = data.get('z', 0)

        # Create a dummy input array matching model input shape
        input_array = np.array([[x, y, z] * 33 + [x]])  # Example: 100 elements if SEQUENCE_LENGTH=100

        prediction = model.predict(input_array)
        predicted_class = int(np.argmax(prediction))
        confidence = round(float(np.max(prediction)) * 100, 2)

        return jsonify({
            "gesture": str(predicted_class),
            "confidence": confidence
        })

    except Exception as e:
        return jsonify({"error": str(e)}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
