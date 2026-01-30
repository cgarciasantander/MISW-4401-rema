Import("env")
import os

# Load .env file
def load_dotenv(filepath=".env"):
    env_vars = {}
    if os.path.exists(filepath):
        with open(filepath) as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith("#") and "=" in line:
                    key, value = line.split("=", 1)
                    env_vars[key.strip()] = value.strip()
    return env_vars

# Read .env file
dotenv = load_dotenv()

# Build flags for string and numeric values
build_flags = []

# String values (need escaped quotes)
string_vars = ["WIFI_SSID", "WIFI_PASSWORD", "HOSTNAME", "MQTT_HOST", "MQTT_USER", "MQTT_PASS"]
for var in string_vars:
    if var in dotenv:
        build_flags.append(f'-D {var}=\\"{dotenv[var]}\\"')

# Numeric values (no quotes)
numeric_vars = ["MQTT_PORT"]
for var in numeric_vars:
    if var in dotenv:
        build_flags.append(f'-D {var}={dotenv[var]}')

# Append to existing build flags
env.Append(BUILD_FLAGS=build_flags)

print("Loaded environment variables from .env:")
for flag in build_flags:
    # Hide passwords in output
    if "PASSWORD" in flag or "PASS" in flag:
        print(f"  {flag.split('=')[0]}=****")
    else:
        print(f"  {flag}")
