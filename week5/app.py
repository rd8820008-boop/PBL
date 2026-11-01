
from flask import Flask, jsonify, render_template, request
from sweep import sweep
from algorithms import run

app = Flask(__name__)


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/api/simulate", methods=["POST"])
def simulate():
    data = request.get_json(silent=True) or {}
    try:
        out = run(
            data.get("reference"),
            data.get("frames"),
            data.get("algorithm", "all")
        )
        return jsonify(out)
    except ValueError as e:
        return jsonify({"error": str(e)}), 400


@app.route("/api/sweep", methods=["POST"])
def sweep_api():
    data = request.get_json(silent=True) or {}
    try:
        return jsonify(sweep(data.get("reference")))
    except ValueError as e:
        return jsonify({"error": str(e)}), 400


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
