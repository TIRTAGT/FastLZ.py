import argparse
import sys
import fastlzpy

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Decompress data using fastlzpy.")
    parser.add_argument("--input", type=str, help="Input data to decompress (hex string). If not provided, data will be read from stdin.")
    parser.add_argument("--stdin", action="store_true", help="Read input data from stdin")
    args = parser.parse_args()
    
    if args.input:
        input_data = bytes.fromhex(args.input)
    elif args.stdin:
        input_data = sys.stdin.buffer.read().strip()

        if not input_data:
            print("No input data provided. Please provide data via --input or stdin.", file=sys.stderr)
            sys.exit(1)
    else:
        print("No input data provided. Please provide data via --input or stdin.", file=sys.stderr)
        sys.exit(1)

    decompressed = fastlzpy.fastlz_decompress_dynamic(input_data)
    print(decompressed)