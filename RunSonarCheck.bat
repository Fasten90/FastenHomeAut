echo Run SonarLint

sonar-scanner.bat -Dsonar.projectKey=FastenHomeAut_SolarTest -Dsonar.organization=fasten90-bitbucket -Dsonar.sources=. -Dsonar.cfamily.build-wrapper-output=bw-output -Dsonar.host.url=https://sonarcloud.io -Dsonar.login=f72d8ba1fd4fb56cad5c7f8329cb7b5378380d6c

pause
