from behave import given, when, then
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains
from feature1_find_products import main_page


@given(u'Shopping cart is empty')
def step_impl(context):
    context.driver.find_element(
        By.XPATH, "//div[@id='header-cart']/div/button")
    context.driver.find_element(
        By.XPATH, "//li[contains(.,'Your shopping cart is empty!')]")


@when(u'User adds featured product to cart')
def step_impl(context):
    context.driver.find_element(
        By.CSS_SELECTOR, ".col:nth-child(1) > .product-thumb button:nth-child(1)").click()


@then(u'User should see notification')
def step_impl(context):
    context.driver.find_element(By.XPATH, "//div[@id='alert']/div").click()


@then(u'Shopping cart should contain one product')
def step_impl(context):
    context.driver.find_element(
        By.XPATH, "//button[contains(.,'1 item(s)')]").click()


@given(u'User is on page with product details')
def product_detail(context):
    context.driver.get("http://opencart:8080/product/iphone")


@given(u'product is in stock')
def step_impl(context):
    context.driver.find_element(
        By.XPATH, "//li[contains(.,'Availability: In Stock')]").click()


@when(u'User adds product to cart')
def detail_add_to_cart(context):
    context.driver.find_element(By.ID, "button-cart").click()


@given(u'Shopping cart is not empty')
def step_impl(context):
    product_detail(context)
    detail_add_to_cart(context)


@when(u'User clicks on shopping cart')
def click_on_cart(context):
    context.driver.find_element(
        By.XPATH, "//*[contains(.,'item(s)')]").click()


@then(u'User should see products in shopping cart')
def step_impl(context):
    context.driver.find_element(
        By.XPATH, "//div[@id='header-cart']/div/ul/li/div/table/tbody/tr[2]/td/strong")


@when(u'User opens shopping cart page')
def open_cart(context):
    click_on_cart(context)
    context.driver.find_element(
        By.XPATH, "//*[contains(.,' View Cart')]").click()


@given(u'User is on shopping cart page')
def step_impl(context):
    main_page(context)
    context.driver.find_element(
        By.XPATH, "//div[@id=\'header-cart\']/div/button").click()
    context.driver.find_element(
        By.CSS_SELECTOR, "strong > .fa-cart-shopping").click()
    context.driver.find_element(
        By.XPATH, "//h1[contains(.,'Shopping Cart')]").click()
    context.driver.find_element(
        By.XPATH, "//tfoot[@id='checkout-total']/tr[4]/td/strong").click()


@when(u'User updates quantity of product to "{amount}"')
def step_impl(context, amount):
    element = context.driver.find_element(By.NAME, "quantity")
    actions = ActionChains(context.driver)
    actions.double_click(element).perform()
    context.driver.find_element(By.NAME, "quantity").send_keys(amount)
    context.driver.find_element(By.XPATH, "//form/div/button/i").click()


@then(u'User should see new quantity "{amount}"')
def step_impl(context, amount):
    value = context.driver.find_element(
        By.NAME, "quantity").get_attribute("value")
    print(value, amount)
    assert value == amount


@when(u'User removes product from shopping cart')
def step_impl(context):
    context.driver.find_element(
        By.XPATH, "//div[@id='shopping-cart']/div/table/tbody/tr/td[4]/form/div/button[2]/i").click()


@then(u'Shopping cart should be empty')
def step_impl(context):
    context.driver.find_element(
        By.XPATH, "//p[contains(.,'Your shopping cart is empty!')]").click()
    context.driver.find_element(By.LINK_TEXT, "Continue").click()


@given(u'User is on checkout page')
def checkout_page(context):
    product_detail(context)
    detail_add_to_cart(context)
    main_page(context)
    context.driver.find_element(
        By.XPATH, "//div[@id=\'header-cart\']/div/button").click()
    context.driver.find_element(
        By.CSS_SELECTOR, "strong > .fa-share").click()


@when(u'User chooses guest checkout')
def guest_checkout(context):
    context.driver.find_element(By.ID, "input-guest").click()


@when(u'User fills out required information')
def personal_info(context):
    context.driver.find_element(
        By.ID, "input-firstname").send_keys("FirstName")
    context.driver.find_element(By.ID, "input-lastname").send_keys("LastName")
    context.driver.find_element(
        By.ID, "input-email").send_keys("mail@example.org")
    context.driver.find_element(
        By.ID, "input-shipping-address-1").send_keys("AddressName")
    context.driver.find_element(
        By.ID, "input-shipping-city").send_keys("CityName")
    dropdown = context.driver.find_element(By.ID, "input-shipping-country")
    dropdown.find_element(By.XPATH, "//option[. = 'Slovak Republic']").click()
    dropdown = context.driver.find_element(By.ID, "input-shipping-zone")
    dropdown.find_element(By.XPATH, "//option[. = 'Košický']").click()


@when(u'Users continues to payment method')
def continue_to_payment(context):
    context.driver.find_element(By.ID, "button-register").click()


@given(u'User has filled out personal information')
def step_impl(context):
    checkout_page(context)
    guest_checkout(context)
    personal_info(context)
    continue_to_payment(context)


@when(u'User chooses shipping method')
def step_impl(context):
    context.driver.find_element(By.ID, "button-shipping-methods").click()
    context.driver.find_element(
        By.ID, "input-shipping-method-flat-flat").click()
    context.driver.find_element(By.ID, "button-shipping-method").click()


@when(u'User chooses payment method')
def step_impl(context):
    context.driver.find_element(By.ID, "button-payment-methods").click()
    context.driver.find_element(By.ID, "input-payment-method-cod-cod").click()
    context.driver.find_element(By.ID, "button-payment-method").click()


@when(u'User confirms order')
def step_impl(context):
    context.driver.find_element(By.ID, "button-confirm").click()


@then(u'User should see order success page')
def step_impl(context):
    context.driver.find_element(
        By.XPATH, "//h1[contains(.,'Your order has been placed!')]").click()
    context.driver.find_element(
        By.XPATH, "//p[contains(.,'Your order has been successfully processed!')]").click()
    context.driver.find_element(By.LINK_TEXT, "Continue").click()
