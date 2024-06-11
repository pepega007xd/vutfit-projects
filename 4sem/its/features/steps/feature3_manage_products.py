from behave import given, when, then
from selenium.webdriver.common.by import By

from time import sleep


@given(u'Admin is on administration dashboard')
def admin_dashboard(context):
    context.driver.get("http://opencart:8080/administration/")
    context.driver.set_window_size(960, 1048)
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.XPATH, "//button[@type=\'submit\']").click()


@when(u'Admin opens page "Products"')
def goto_products_page(context):
    context.driver.find_element(
        By.XPATH, "//button[@id=\'button-menu\']/i").click()
    sleep(1)
    context.driver.find_element(By.LINK_TEXT, "Catalog").click()
    context.driver.find_element(By.LINK_TEXT, "Products").click()


@then(u'Admin should see list of products')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".col .card-header").click()
    context.driver.find_element(
        By.XPATH, "//form[@id=\'form-product\']/div/table/tbody/tr/td[3]").click()


@given(u'Admin is on products page')
def step_impl(context):
    admin_dashboard(context)
    goto_products_page(context)


@when(u'Admin sets quantity filter to "{amount}"')
def step_impl(context, amount):
    context.driver.find_element(
        By.CSS_SELECTOR, ".d-lg-none > .fa-filter").click()
    context.driver.find_element(By.ID, "input-quantity").send_keys(amount)
    context.driver.find_element(
        By.XPATH, "//button[@id=\'button-filter\']/i").click()


@then(u'Admin should see list of products with quantity "{amount}"')
def step_impl(context, amount):
    elements = context.driver.find_elements(
        By.XPATH, f"//span[contains(.,'{amount}')]")
    assert len(elements) > 0


@when(u'Admin creates new product with name "{name}"')
def step_impl(context, name):
    context.driver.find_element(
        By.XPATH, "//div[@id=\'content\']/div/div/div/a").click()
    context.driver.find_element(
        By.ID, "input-name-1").send_keys(name)


@when(u'Admin sets required product info')
def step_impl(context):
    context.driver.find_element(
        By.ID, "input-meta-title-1").send_keys("meta_tag")
    context.driver.find_element(By.LINK_TEXT, "Data").click()
    context.driver.find_element(By.ID, "input-model").send_keys("model_name")
    context.driver.find_element(By.LINK_TEXT, "SEO").click()
    context.driver.find_element(
        By.ID, "input-keyword-0-1").send_keys("keyword")


@when(u'Admin saves and closes the product')
def step_impl(context):
    context.driver.find_element(By.XPATH, "//button[@type=\'submit\']").click()
    goto_products_page(context)


@then(u'Admin should see notification')
def step_impl(context):
    context.driver.find_element(By.XPATH, "//div[@id='alert']/div").click()


@when(u'Admin selects a product')
def step_impl(context):
    context.driver.find_element(
        By.CSS_SELECTOR, "tbody > tr:nth-child(1) .form-check-input").click()


@when(u'Admin deletes selected products')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".fa-trash-can").click()
    assert context.driver.switch_to.alert.text == "Are you sure?"
    context.driver.switch_to.alert.accept()


@when(u'Admin opens product for editing')
def step_impl(context):
    context.driver.find_element(
        By.CSS_SELECTOR, "tr:nth-child(1) .btn:nth-child(1)").click()


@when(u'Admin changes product name to "{name}"')
def step_impl(context, name):
    context.driver.find_element(By.ID, "input-name-1").clear()
    context.driver.find_element(By.ID, "input-name-1").send_keys(name)


@then(u'Page should contain product with name "{name}"')
def step_impl(context, name):
    products = context.driver.find_elements(
        By.CSS_SELECTOR, "tbody > tr > .text-start:nth-child(3)")
    print(list(map(lambda product: product.text, products)))
    assert any(map(lambda product: product.text.startswith(name), products))


@when(u'Admin changes product quantity to "{amount}"')
def step_impl(context, amount):
    context.driver.find_element(By.LINK_TEXT, "Data").click()
    context.driver.find_element(By.ID, "input-quantity").clear()
    context.driver.find_element(By.ID, "input-quantity").send_keys(amount)


@then(u'Product should have quantity "{amount}"')
def step_impl(context, amount):
    quantities = context.driver.find_elements(
        By.CSS_SELECTOR, "tr:nth-child(1) .badge")
    print(list(map(lambda quantity: quantity.text, quantities)))
    assert any(map(lambda quantity: quantity.text == amount, quantities))
