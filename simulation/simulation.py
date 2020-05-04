#!/usr/bin/python3
import argparse
import sys
import curses
from core.settings import Settings
from core.simulation import Simulation, build_simulations
from core import screen


PY_VERSION = (3, 6)
if not sys.version_info >= PY_VERSION:
    print("Python %d.%d or higher is required." % PY_VERSION)
    print("You are using Python %d.%d.%d." % sys.version_info[:3])
    sys.exit(1)


parser = argparse.ArgumentParser(description='Best simulator in town!')
parser.add_argument('-f', '--fake', action='store_true',
                    help='Print out the commands but does not run')
parser.add_argument('-x', '--use-tui', action='store_true',
                    help='Experimental Text User Interface')
parser.add_argument('-1', '--once', action='store_true',
                    help='Only runs one simulation and stops.')
parser.add_argument('-S', '--settings', default=None,
                    help='Dotted path to the settings module.')
parser.add_argument('-o', '--display-stdout', action='store_true',
                    help='If enabled, display stdout regardless of '\
                        'whichever redirection defined in settings.')



def tui(stdscr):
    curses.start_color()
    curses.init_pair(1, curses.COLOR_BLUE, curses.COLOR_BLACK)
    curses.init_pair(2, curses.COLOR_YELLOW, curses.COLOR_BLACK)
    curses.init_pair(3, curses.COLOR_GREEN, curses.COLOR_BLACK)
    total_jobs = len(Simulation.instances_status)
    s = screen.SimulationScreen(stdscr, total_jobs)
    s.loop(settings.PARALLEL_INSTANCES)



if __name__ == "__main__":
    options = parser.parse_args()    
    settings_module = options.settings or 'settings'
    settings = Settings(settings_module)
    if options.display_stdout:
        setattr(settings, 'STDOUT', None)
    build_simulations(settings, options=options)
    if options.use_tui:
        curses.wrapper(tui)
    else:
        Simulation.dispatch_all(settings.PARALLEL_INSTANCES)
        Simulation.join()
