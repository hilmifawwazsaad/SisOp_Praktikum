# Use PHP 8.1 FPM as base image
FROM php:8.1-fpm

# Install system dependencies
RUN apt-get update && apt-get install -y \
    libpng-dev \
    libonig-dev \
    libxml2-dev \
    zip \
    unzip \
    curl

# Install PHP extensions
RUN docker-php-ext-install pdo_mysql mbstring exif pcntl bcmath gd

# Set working directory
WORKDIR /var/www/html

# Install Composer
RUN curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer

# Ensure Composer runs as non-root for security reasons (setting environment variable to avoid warnings)
ENV COMPOSER_ALLOW_SUPERUSER=0

# Add user for application
RUN groupadd -g 1000 www && \
    useradd -u 1000 -ms /bin/bash -g www www

# Copy the application code to the image
COPY . /var/www/html

# Change owner of the copied files to the new 'www' user
RUN chown -R www:www /var/www/html

# Run Composer as 'www' user
USER www

# Install project dependencies
RUN composer install

# Switch back to root user to run the application
USER root

# Expose port 8000
EXPOSE 8000

# Set the ENTRYPOINT to use PHP's built-in server
CMD ["php", "artisan", "serve", "--host=0.0.0.0", "--port=8000"]
