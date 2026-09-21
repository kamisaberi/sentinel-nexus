#!/usr/bin/env python3
"""
Sentinel Nexus: Sneakernet Telemetry Exporter
Packages high-uncertainty candidate batches into an offline bundle for xinfer-forge.
"""

import os
import sys
import glob
import time
import tarfile
import hashlib
import json

BUFFER_DIR = "/var/lib/sentinel-nexus/forge_buffer"
FALLBACK_DIR = "/tmp/sentinel_nexus_forge"
OUTPUT_DIR = "./sneakernet_bundles"

def main():
    target_dir = BUFFER_DIR if os.path.exists(BUFFER_DIR) else FALLBACK_DIR
    if not os.path.exists(target_dir):
        print(f"[-] Buffer directory not found: {target_dir}")
        sys.exit(1)

    batch_files = glob.glob(os.path.join(target_dir, "candidate_batch_*.bin"))
    if not batch_files:
        print("[*] No candidate vector batches found to export.")
        sys.exit(0)

    os.makedirs(OUTPUT_DIR, exist_ok=True)
    timestamp = int(time.time())
    bundle_filename = f"nexus_telemetry_bundle_{timestamp}.snbundle"
    bundle_path = os.path.join(OUTPUT_DIR, bundle_filename)

    print(f"[*] Packaging {len(batch_files)} candidate batches into: {bundle_path}")

    manifest = {
        "version": "1.0",
        "created_at_utc": time.asctime(time.gmtime()),
        "batch_count": len(batch_files),
        "files": []
    }

    with tarfile.open(bundle_path, "w:gz") as tar:
        for f in batch_files:
            fname = os.path.basename(f)
            # Compute sha256
            hasher = hashlib.sha256()
            with open(f, "rb") as bin_f:
                buf = bin_f.read()
                hasher.update(buf)
            manifest["files"].append({
                "filename": fname,
                "sha256": hasher.hexdigest(),
                "size_bytes": len(buf)
            })
            tar.add(f, arcname=fname)

        # Write manifest inside archive
        manifest_data = json.dumps(manifest, indent=2).encode('utf-8')
        tarinfo = tarfile.TarInfo(name="manifest.json")
        tarinfo.size = len(manifest_data)
        import io
        tar.addfile(tarinfo, io.BytesIO(manifest_data))

    # Calculate bundle sha256
    bundle_hash = hashlib.sha256(open(bundle_path, "rb").read()).hexdigest()
    print(f"[+] Bundle successfully generated!")
    print(f"    Size: {os.path.getsize(bundle_path):,} bytes")
    print(f"    SHA256: {bundle_hash}")
    print(f"[!] You can now copy this bundle to a secure USB drive for xinfer-forge retraining.")

if __name__ == "__main__":
    main()