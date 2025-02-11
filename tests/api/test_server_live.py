# Copyright 2022 Xilinx Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import pytest


@pytest.mark.usefixtures("server")
class TestServerLive:
    """
    Base class for serverLive tests using the Python bindings
    """

    def test_server_live(self):
        """
        Test serverLive
        """

        assert self.rest_client.serverLive()


# not using the server fixture here
class TestServerNotLive:
    """
    Base class for serverLive tests using the Python bindings
    """

    def test_server_live(self):
        """
        Test serverLive
        """

        assert not self.rest_client.serverLive()
