import os
import subprocess
import time
import random
import logging
from itertools import count
from multiprocessing import Process, Queue, Manager


class Simulation:
    _id_seed = count()
    instances = Queue()
    instances_status = {}
    status = None

    def __init__(self, executable, args, stdin=None, stdout=None, stderr=None, fake=None):
        self.executable = executable
        self.args = args
        self.stdin = stdin
        self.stdout = stdout
        self.stderr = stderr
        self.fake = fake
        self.id = next(Simulation._id_seed)

        Simulation.instances.put(self)
        Simulation.instances_status.update({
            self.id: {
                'running': False,
                'done': False,
                'time_start': None,
                'time_end': None,
                'worker_pid': None,
                'instance': self,
            }
        })

    def parse(self):
        line = f'{self.executable}'
        if self.stdin:
            line += f' <{self.stdin}'
        if self.stdout:
            line += f' >{self.stdout}'
        if self.stderr:
            line += f' 2>{self.stderr}'
        if self.args:
            line += ' ' + ' '.join(f'{k} {v}' for k, v in self.args)
        return line

    @classmethod
    def dispatch_all(cls, parallel_instances=1):
        cls.process_lst = []
        cls.manager = Manager()
        Simulation.status = cls.manager.dict()
        Simulation.status.update(Simulation.instances_status)

        for cpu in range(parallel_instances):
            p = Process(target=cls._run, args=(Simulation.status, cpu, ))
            cls.process_lst.append(p)
            p.start()

    @classmethod
    def join(cls):
        for p in cls.process_lst:
            p.join()

    @classmethod
    def _run(cls, status, cpu_affinity=None):
        if cpu_affinity:
            os.sched_setaffinity(0, [cpu_affinity])

        while True:
            try:
                instance = cls.instances.get_nowait()
            except:
                return

            args = map(str, sum(instance.args, ()))
            process_info = [instance.executable, *args]

            if not instance.fake:
                open_files = {}
                for stream in ['stdin', 'stdout', 'stderr']:
                    file_stream = getattr(instance, stream)
                    if file_stream:
                        os.makedirs(os.path.dirname(
                            file_stream), exist_ok=True)
                        open_files.setdefault(stream, open(file_stream, 'w'))
                val = status[instance.id]
                val.update({
                    'running': True,
                    'time_start': time.time()
                })
                status[instance.id] = val
                subprocess.call(process_info, **open_files)
                val = status[instance.id]
                val.update({
                    'running': False,
                    'done': True,
                    'time_end': time.time(),
                })
                status[instance.id] = val

                for f in open_files.values():
                    f.close()
            else:
                val = status[instance.id]
                val.update({
                    'running': True,
                    'time_start': time.time()
                })
                status[instance.id] = val
                subprocess.call(
                    ['python3 -c "import time; import random; time.sleep(random.randrange(2, 8))"'], shell=True)
                val = status[instance.id]
                val.update({
                    'running': False,
                    'done': True,
                    'time_end': time.time(),
                })
                status[instance.id] = val


def build_simulations(settings, options):
    simulations = []
    for args in settings.args:
        parsed_values, args = settings.parse_variables(args)

        if not options.fake:
            # Create -output path if it does not exist
            os.makedirs(args['-output'], exist_ok=True)

        sim = Simulation(settings.EXECUTABLE,
                         list(args.items()),
                         stdin=parsed_values['STDIN'],
                         stdout=parsed_values['STDOUT'],
                         stderr=parsed_values['STDERR'],
                         fake=options.fake)
        simulations.append(sim)

        # only runs one simulation
        if options.once:
            break
    return simulations
