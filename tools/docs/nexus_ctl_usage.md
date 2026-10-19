### How to Use

1. **Register a New User Account:**
   ```bash
   nexus-ctl auth register kamisaberi@gmail.com 12345678 kamisaberi --backend-url https://backend.aryorithm.com
   # Or for local development:
   nexus-ctl auth register kamisaberi@gmail.com 12345678 kamisaberi --backend-url http://localhost:8000
   ```

2. **Login to Existing Account:**
   ```bash
   nexus-ctl auth login kamisaberi@gmail.com 12345678 --backend-url https://backend.aryorithm.com
   ```

3. **Check Auth Status:**
   ```bash
   nexus-ctl auth status
   ```

4. **Publish Artifact Automatically (Uses saved API Key in `~/.sentinel/credentials.json`):**
   ```bash
   sentinel-artifact publish plant_safety_filter.spkg
   ```