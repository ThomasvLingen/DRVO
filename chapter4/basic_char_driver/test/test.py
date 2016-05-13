from unittest import TestCase
import io
import os

class driverTest(TestCase):
    driver_path = "/dev/character_device"
    UNBUFFERED = 0

    def _executeCommand(self, the_command):
        temp_list = os.popen(the_command).read()
        return temp_list

    def _getDMESG(self):
        message = self._executeCommand("dmesg | tail -n 1")
        return self._remove_timestamp(message)

    def _remove_timestamp(self, dmesg_message):
        end_index = dmesg_message.find(']')
        return dmesg_message[end_index+2:]

    def _open_driver(self, mode):
        if mode == 'wb':
            return io.open("/dev/character_driver", mode, self.UNBUFFERED)
        else:
            return io.open("/dev/character_driver", mode)

    def test_open_close(self):
        driver = self._open_driver('r')
        self.assertEquals(self._getDMESG(), "char driver is opening!\n")
        driver.close()
        self.assertEquals(self._getDMESG(), "char driver is closing!\n")

    def test_read(self):
        driver = self._open_driver('rb')
        self.assertEquals(driver.read(), "")
        self.assertEquals(self._getDMESG(), "char driver READ is not implemented\n")

    def test_write(self):
        """
        writing to the driver should return the length of the written message
        """
        driver = self._open_driver('wb')

        msg_5 = "12345"
        msg_10 = "1234512345"

        self.assertEquals(driver.write(msg_5), len(msg_5))
        self.assertEquals(self._getDMESG(), "char driver WRITE is not implemented\n")
        self.assertEquals(driver.write(msg_10), len(msg_10))
        self.assertEquals(self._getDMESG(), "char driver WRITE is not implemented\n")
