from behave import given, when, then
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.by import By


@given(u'User is on main page')
def main_page(context):
    context.driver.get("http://opencart:8080/")


@then(u'User should see featured products')
def step_impl(context):
    context.driver.find_element(
        By.XPATH, "//h3[contains(.,'Featured')]").click()
    context.driver.find_element(
        By.XPATH, "//div[@id='content']/div[2]/div[2]/div/div[2]/div/p").click()
    context.driver.find_element(By.LINK_TEXT, "MacBook").click()


@when(u'User opens category "{category}"')
def step_impl(context, category):
    context.driver.find_element(By.LINK_TEXT, category).click()


@when(u'User opens the entire category "{category}"')
def step_impl(context, category):
    context.driver.find_element(By.LINK_TEXT, category).click()
    context.driver.find_element(By.LINK_TEXT, f"Show All {category}").click()


@then(u'User should see products from category "{category}"')
def step_impl(context, category):
    context.driver.find_element(
        By.XPATH, f"//h2[contains(.,'{category}')]").click()
    context.driver.find_element(
        By.CSS_SELECTOR, ".col:nth-child(2) > .product-thumb p").click()
    context.driver.find_element(By.LINK_TEXT, "Apple Cinema 30\"").click()


@when(u'User opens subcategory "{subcategory}"')
def step_impl(context, subcategory):
    context.driver.find_element(
        By.XPATH, f"//a[contains(text(),'{subcategory}')]").click()


# page doesnt' contain any identifiable text
# to make this generic over subcategories
@then(u'User should see products from subcategory "Monitors"')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".col:nth-child(1) p").click()
    context.driver.find_element(
        By.LINK_TEXT, "Samsung SyncMaster 941BW").click()


@then(u'User should see empty category page')
def step_impl(context):
    context.driver.find_element(
        By.XPATH, "//p[contains(.,'There are no products to list in this category.')]").click()
    context.driver.find_element(By.LINK_TEXT, "Continue").click()


@when(u'User searches "{search_term}"')
def step_impl(context, search_term):
    context.driver.find_element(By.NAME, "search").click()
    context.driver.find_element(By.NAME, "search").send_keys(search_term)
    context.driver.find_element(By.NAME, "search").send_keys(Keys.ENTER)
    context.driver.find_element(By.ID, "button-search").click()


@then(u'User should see product with name matching "{name}"')
def step_impl(context, name):
    elements = context.driver.find_elements(
        By.XPATH, f"//a[contains(text(),'{name}')]")
    assert len(elements) > 0


@when(u'User sets search category to "{category}"')
def step_impl(context, category):
    dropdown = context.driver.find_element(By.ID, "input-category")
    dropdown.find_element(By.XPATH, f"//option[. = '{category}']").click()
    context.driver.find_element(By.ID, "button-search").click()


@then(u'User should not see product with name matching "{name}"')
def step_impl(context, name):
    elements = context.driver.find_elements(By.LINK_TEXT, name)
    assert len(elements) == 0
