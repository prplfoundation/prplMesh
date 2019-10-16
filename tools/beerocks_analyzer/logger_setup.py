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
        logging.setLogRecordFactory(FormatRecordFactory)


class FormatRecordFactory(logging.LogRecord):
    """A factory which formats messages with str.format."""
    def getMessage(self):
        """
        Return the message for this LogRecord, formatted with str.format.

        Return the message for this LogRecord after merging any user-supplied
        arguments with the message by using str.format.
        """
        msg = str(self.msg)
        if self.args:
            msg = msg.format(self.args)
        return msg
