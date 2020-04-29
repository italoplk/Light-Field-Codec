import argparse
from core.settings import Settings
from core.simulation import Simulation, build_simulations

"""
python simulation.py --fake
python simulation.py --once

"""


parser = argparse.ArgumentParser(description='Best simulator in town!')
parser.add_argument('-f', '--fake', action='store_true',
                    help='Print out the commands but does not run')
parser.add_argument('-1', '--once', action='store_true',
                    help='Only runs one simulation and stops.')


if __name__ == "__main__":
    options = parser.parse_args()
    settings = Settings('settings')
    build_simulations(settings, options=options)
    Simulation.dispatch_all(settings.PARALLEL_INSTANCES)
