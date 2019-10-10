import logging
import logging.config
import yaml


class StderrFilter(logging.Filter):
    """Simple filter which only outputs the following levels: WARNING, ERROR, CRITICAL.
    """

    def filter(self, rec):
        return rec.levelno in (logging.WARNING, logging.ERROR, logging.CRITICAL)


def setup_logger():
    with open('logger_config.yaml', 'rt') as file:
        config = yaml.safe_load(file.read())
        logging.config.dictConfig(config)
