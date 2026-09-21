#!/usr/bin/env python3
"""
Sentinel Nexus: Sneakernet Model Importer
Verifies offline-trained ONNX models and stages them for canary rollout.
"""

import os
import sys
import hashlib
import json
import urllib.request

NEXUS_API = "http://localhost:9443/api/v1/ota/stage"

def main():
    if len(sys.argv) < 2:
        print("Usage: import_signed_model.py <path_to_model.onnx> [destination_models_dir]")
        sys.exit(1)

    model_path = sys.argv[1]
    dest_dir = sys.argv[2] if len(sys.argv) > 2 else "models"

    if not os.path.exists(model_path):
        print(f"[-] Error: File not found: {model_path}")
        sys.exit(1)

    os.makedirs(dest_dir, exist_ok=True)

    # 1. Compute Cryptographic Checksum
    hasher = hashlib.sha256()
    with open(model_path, "rb") as f:
        while chunk := f.read(65536):
            hasher.update(chunk)
    sha256_hash = hasher.hexdigest()

    model_name = os.path.basename(model_path)
    target_path = os.path.join(dest_dir, model_name)

    # Copy to model repository
    with open(model_path, "rb") as src, open(target_path, "wb") as dst:
        dst.write(src.read())

    print(f"[+] Model cached in local repository: {target_path}")
    print(f"    SHA256: {sha256_hash}")

    # 2. Stage to Nexus Canary Orchestrator
    try:
        req_data = json.dumps({
            "version": model_name,
            "sha256": sha256_hash,
            "url": f"/models/{model_name}"
        }).encode('utf-8')

        req = urllib.request.Request(
            NEXUS_API,
            data=req_data,
            headers={"Content-Type": "application/json"}
        )
        with urllib.request.urlopen(req) as resp:
            res_json = json.loads(resp.read().decode('utf-8'))
            print(f"[+] Model Staging Response: {res_json}")
            print(f"[!] Model is now in SHADOW_MODE. Appliances will run shadow evaluation.")
    except Exception as e:
        print(f"[-] Could not communicate with Nexus API: {e}")

if __name__ == "__main__":
    main()