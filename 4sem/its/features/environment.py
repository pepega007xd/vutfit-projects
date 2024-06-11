from behave import fixture, use_fixture
from selenium import webdriver
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities


@fixture
def selenium_browser_chrome(context):
    context.driver = webdriver.Remote(
        command_executor='http://localhost:4444/wd/hub',
        desired_capabilities=DesiredCapabilities.CHROME)

    context.driver.implicitly_wait(15)
    context.driver.set_window_size(1280, 1408)

    yield context.driver

    context.driver.quit()


def before_feature(context, feature):
    use_fixture(selenium_browser_chrome, context)


def before_scenario(context, scenario):
    context.driver.delete_all_cookies()
