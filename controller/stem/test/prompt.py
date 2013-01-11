"""
Simple helper methods to make troubleshooting with the python interpreter
easier.

::

  >>> from test.prompt import *
  >>> controller = controller()
  >>> controller.get_info("version")
  '0.2.1.30'

  >>> is_running()
  True

  >>> stop()
"""

import os
import signal
import sys

import stem.control
import stem.process
import stem.util.system

CONTROL_PORT = 2779

STOP_CONFIRMATION = "Would you like to stop the tor instance we made? (y/n, default: n): "


def print_usage():
  """
  Provides a welcoming message.
  """

  print "Welcome to stem's testing prompt. You currently have a controller available"
  print "via the 'controller' variable."
  print


def start(orType = 1):
  """
  Starts up a tor instance that we can attach a controller to.
  """

  tor_config = None
  if orType == 1: #OP
    tor_config = {
      'SocksListenAddress': '127.0.0.1',
      'SocksPort' : '9050',
      'ControlPort': str(CONTROL_PORT),
      'ExitPolicy': 'reject *:*',
      'UseN23' : '1',
      'N3Initial': '500',
      'N3Max' : '500',
      'N3Min': '100',
      '__DisablePredictedCircuits' : '1',
      'MaxOnionsPending' : '0',
      'newcircuitperiod' : '999999999',
      'maxcircuitdirtiness' : '999999999',
      'TestingTorNetwork' : '1',
      'ServerDNSDetectHijacking' : '0',
      'DirServer' : 'auth1 v3ident=43580C5C0C3308CEE06162D3C97F291364A00E31 10.1.3.2:9030 62C78AE9C12932E66EDB8F808848B19896B3EF86',
      'DataDirectory' : '/opt/tor',
    }
  elif orType == 2: #Entry
    tor_config = {
        'SocksListenAddress': '127.0.0.1',
        'SocksPort': '9050',
        'ORPort' : '9001',
        'DirPort': '0',
        'RelayBandwidthRate' : '100 KBytes',
        'RelayBandwidthBurst' : '200 KBytes',
        'ControlPort': str(CONTROL_PORT),
        'UseN23' : '1',
        'N3Initial': '500',
        'N3Max' : '500',
        'N3Min': '100',
        '__DisablePredictedCircuits' : '1',
        'MaxOnionsPending' : '0',
        'newcircuitperiod' : '999999999',
        'maxcircuitdirtiness' : '999999999',
        'Nickname' : 'N23TestEntry',
        'ExitPolicy': 'accept *:80, reject *:*',
        'ContactInfo': 'bwrichte at princeton dot edu',
        'TestingTorNetwork' : '1',
        'ServerDNSDetectHijacking' : '0',
        'DirServer' : 'auth1 v3ident=43580C5C0C3308CEE06162D3C97F291364A00E31 10.1.3.2:9030 62C78AE9C12932E66EDB8F808848B19896B3EF86',
        'DataDirectory' : '/opt/tor',
    }
  elif orType == 3: #Middle
    tor_config = {
        'SocksListenAddress': '127.0.0.1',
        'SocksPort': '9050',
        'ORPort' : '9001',
        'ControlPort': str(CONTROL_PORT),
        'DirPort': '0',
        'UseN23' : '1',
        'N3Initial': '500',
        'N3Max' : '500',
        'N3Min': '100',
        '__DisablePredictedCircuits' : '1',
        'MaxOnionsPending' : '0',
        'newcircuitperiod' : '999999999',
        'maxcircuitdirtiness' : '999999999',
        'Nickname' : 'N23TestMiddle',
        'ExitPolicy': 'accept *:80, reject *:*',
        'ContactInfo': 'bwrichte at princeton dot edu',
        'TestingTorNetwork' : '1',
        'ServerDNSDetectHijacking' : '0',
        'DirServer' : 'auth1 v3ident=43580C5C0C3308CEE06162D3C97F291364A00E31 10.1.3.2:9030 62C78AE9C12932E66EDB8F808848B19896B3EF86',
        'DataDirectory' : '/opt/tor',
    }
  elif orType == 4: #Exit
    tor_config = {
        'SocksListenAddress': '127.0.0.1',
        'SocksPort': '9050',
        'ORPort' : '9001',
        'ControlPort': str(CONTROL_PORT),
        'DirPort': '0',
        'UseN23' : '1',
        'N3Initial': '500',
        'N3Max' : '500',
        'N3Min': '100',
        '__DisablePredictedCircuits' : '1',
        'MaxOnionsPending' : '0',
        'newcircuitperiod' : '999999999',
        'maxcircuitdirtiness' : '999999999',
        'Nickname' : 'N23TestExit',
        'ExitPolicy': 'accept *:80, reject *:*',
        'ContactInfo': 'bwrichte at princeton dot edu',
        'TestingTorNetwork' : '1',
        'ServerDNSDetectHijacking' : '0',
        'DirServer' : 'auth1 v3ident=43580C5C0C3308CEE06162D3C97F291364A00E31 10.1.3.2:9030 62C78AE9C12932E66EDB8F808848B19896B3EF86',
        'DataDirectory' : '/opt/tor',
    }
  else:
    raise OSError('Ah hell no')

  sys.stdout.write("Starting tor...")
  stem.process.launch_tor_with_config(config = tor_config, completion_percent = 50)
  sys.stdout.write("  done\n\n")


def stop(prompt = False):
  """
  Stops the tor instance spawned by this module.

  :param bool prompt: asks user for confirmation that they would like to stop tor if True
  """

  tor_pid = stem.util.system.get_pid_by_port(CONTROL_PORT)

  if tor_pid:
    if prompt:
      response = raw_input("\n" + STOP_CONFIRMATION)

      if not response.lower() in ("y", "yes"):
        return

    os.kill(tor_pid, signal.SIGTERM)


def is_running():
  """
  Checks if we're likely running a tor instance spawned by this module. This is
  simply a check if our custom control port is in use, so it can be confused by
  other applications (not likely, but possible).

  :returns: True if the control port is used, False otherwise
  """

  return bool(stem.util.system.get_pid_by_port(CONTROL_PORT))


def controller(orType = 1):
  """
  Provides a Controller for our tor instance. This starts tor if it isn't
  already running.
  """

  if not is_running():
    start(orType)

  controller = stem.control.Controller.from_port(control_port = CONTROL_PORT)
  controller.authenticate()
  return controller
