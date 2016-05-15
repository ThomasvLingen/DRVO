from unittest import TestCase
import io
import os
import time

class driverTest(TestCase):
    driver_path = "/dev/character_device"
    UNBUFFERED = 0

    @classmethod
    def setUpClass(cls):
        # Make sure previous module is unloaded
        os.popen("make remove").read()

    def setUp(self):
        self._executeCommand("make")
        self._executeCommand("make insert")

    def tearDown(self):
        self._executeCommand("make remove")

    def _executeCommand(self, the_command):
        temp_list = os.popen(the_command).read()
        return temp_list

    def _getDMESG(self, lines):
        message = self._executeCommand("dmesg -t | tail -n {}".format(lines))
        print(repr(message))
        messages = message.split('\n')
        print(repr(messages))
        return [
            message for message in messages
            if message != ''
        ]

    def _open_driver(self, mode):
        if mode == 'wb':
            return io.open("/dev/lab1_driver", mode, self.UNBUFFERED)
        else:
            return io.open("/dev/lab1_driver", mode)

    def test_open(self):
        for i in range(0,5):
            driver = self._open_driver('r')
            self.assertEquals(self._getDMESG(2), [
                "lab1_driver is opening [{}]!".format(i),
                "major/minor: 700/0"
            ])
            driver.close()
