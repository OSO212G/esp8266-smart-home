"""Narrow source-contract checks; not firmware runtime coverage.
Run from the repository root: python tests/check_source.py [pins|diagnostic].
"""
from pathlib import Path
import re
import sys
import unittest

SOURCE = Path(__file__).resolve().parents[1] / "firmware/SmartHomePrototype/SmartHomePrototype.ino"


class SourceChecks(unittest.TestCase):
    def test_pins(self):
        source = SOURCE.read_text(encoding="utf-8")
        for name, gpio, label in [("ledPin", 13, "D7"), ("FanPin", 12, "D6"), ("servoPin", 0, "D3")]:
            self.assertRegex(source, rf"int {name} = {gpio}; // [^\n]*GPIO{gpio}\b[^\n]*\b{label}\b")
        self.assertRegex(source, r"myservo.attach\(servoPin\); // [^\n]*GPIO0\b[^\n]*\bD3\b")

    def test_diagnostic(self):
        source = SOURCE.read_text(encoding="utf-8")
        # Require distinct messages inside the result guard after outputs go LOW.
        off = source.index('digitalWrite(FanPin, LOW);')
        tail = source[off:source.index('delay(15000);', off)]
        self.assertRegex(tail, r'if \(result != 0\)\s*\{\s*Serial.println\("Fan and LED turned OFF \(sensor read error\)"\);\s*\} else \{\s*Serial.println\("Fan and LED turned OFF \(temperature <= 24 C\)"\);\s*\}')
        self.assertNotIn("Temperature reached below", source)


if __name__ == "__main__":
    selected = sys.argv[1:]
    suite = unittest.TestSuite(SourceChecks("test_" + name) for name in selected) if selected else unittest.defaultTestLoader.loadTestsFromTestCase(SourceChecks)
    result = unittest.TextTestRunner(verbosity=2).run(suite)
    raise SystemExit(0 if result.wasSuccessful() else 1)
